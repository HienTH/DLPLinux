/* Summary: Gets Linux runtime information from VSF virtual file system. 
 *
 */
#include <iostream>
#include <fstream> 
#include <sstream>
#include <string> 
#include <iomanip>  // std::seprecision, std::fixed 

// See: https://man7.org/linux/man-pages/man2/sysinfo.2.html
#include <sys/sysinfo.h>

void        get_memory_info();
void        show_sysinfo1();
void        show_uptime();
void        show_mount_points();
void 	    show_kernel_runtime_config();
std::string readFile(std::string const& file);

int main(int argc, char** argv)
{
	if(argc < 2){
		std::cerr << " [ERROR] Usage: $ " << argv[0] << " <COMMAND> " << '\n';
		return EXIT_FAILURE;
	}

	auto command = std::string{ argv[1] };

	if(command == "-h") {
		std::cout << " Tham so hien thi thong tin phan cung may tinh: \n";
		printf("	version: Hien thi version cua he thong.\n");
		printf("	memory: Hien thi thong tin bo nho.\n");
		printf("	sysinfo1: Hien thi Thong tin bo nho RAM, SWAP, So luong tien trinh dang chay, Time boot.\n");
		printf("	uptime: Hien thi Time boot.\n");
		printf("        load-avg: Hien thi Thong tin tai he thong.\n");
		printf("        mount: Hien thi Thong tin mount.\n");
		printf("        sysctl: Hien thi Thong tin Hostname, Version, OS,...\n");
		printf("        dmesg: Hien thi Thong tin Kernel debug message.\n");
		printf("        cpuinfo: Hien thi Thong tin CPU.\n");

	}

	if(command == "version"){
		std::cout << "===== Kernel Version ====================================" << '\n';
		auto version = readFile("/proc/version");
		std::cout << " System version = " << version << '\n';
		return EXIT_SUCCESS;
	}

 	if(command == "memory")
	{
		std::cout << " ==== Memory (Reported by command $ free -m ) ========" << '\n';
		get_memory_info();
		return EXIT_SUCCESS;
	 }

	// Using the header: <sys/sysinfo.h>  
	// Note: this information is reported by the command $ free -m 
	if(command == "sysinfo1")
	{
		std::cout << " ===== System Info 1 (Reported by $ free -m and $ uptime ) === " << '\n';
		show_sysinfo1();
		return EXIT_SUCCESS;
	}

	if(command == "uptime")
	{
		std::cout << "===== System uptime since boot (Reported by $ uptime) ====" << '\n';
		show_uptime();
		return EXIT_SUCCESS;
	}

	if(command == "load-avg")
	{
		std::cout << " ==== System Load Average ================================" << '\n';
		auto loadavg = readFile("/proc/loadavg");
		std::cout << "Load average = " << loadavg << '\n';	
		return EXIT_SUCCESS;
	}

	if(command == "mount")
	{
		std::cout << " ====== System mount points ( Reported by command $ mount ) ===== " << '\n';
		show_mount_points();
		return EXIT_SUCCESS;
	}

	if(command == "sysctl")
	{
		std::cout << " ===== Kernel Runtime Information (Reported by $ sysctl ) ======= " << '\n';
		show_kernel_runtime_config();
		return EXIT_SUCCESS;
	}

	if(command == "dmesg")
	{
		std::cout << " ===== Kernel debug message (Reported by $ dmesg ) ==========" << '\n';
		auto ifs = std::ifstream("/dev/kmsg");
		std::string line;
		while( std::getline(ifs, line) ){
			std::cout << line << '\n';
		}
	}

	if(command == "cpuinfo")
	{
		std::cout << "======= CPU Information =====================================" << '\n';
		
		auto ifs = std::ifstream("/proc/cpuinfo");
		std::string line;
		while( std::getline(ifs, line) ){
			std::cout << line << '\n';
		}		
	}

	return EXIT_SUCCESS;
}

void get_memory_info()
{
	constexpr const char* meminfo_file 		= "/proc/meminfo";
	// factor = 1 Gigabytes = 1024 * 1024 kbytes 
	constexpr double factor = 1024 * 1024;
	
	auto ifs = std::ifstream{meminfo_file};
	if(!ifs.good()){
		throw std::runtime_error("Error: unable to memory-info file.");
	}
	std::string line, label;
	std::uint64_t value; 
	while( std::getline(ifs, line) )
	{		
		std::stringstream ss{line};	
		ss >> label >> value;

		if(label == "MemTotal:")
			std::cout << "     Total memory (Gb) = " << (value / factor) << '\n';

		if(label == "MemAvailable:")		
			std::cout << " Memory Available (Gb) = " << (value / factor) << '\n';
	}
}

void show_sysinfo1()
{
	struct sysinfo info;
	::sysinfo(&info);

	// 1 Gigabyte = 1024 megabytes = 1024 * 1024 kbytes = 1024 * 1024 * 1024 bytes;
	constexpr double factor = 1024 * 1024 * 1024;
	constexpr std::uint64_t one_day_to_seconds = 24 * 60 * 60;

	std::cout << " [*] System uptime since boot (seconds) = " << info.uptime << '\n'
		   	  << " [*] System uptime since boot    (days) = " << info.uptime    / one_day_to_seconds << '\n'
		  	  << " [*]              Total RAM memory (Gb) = " << info.totalram  / factor << '\n'
			  << " [*]              Free  RAM memory (Gb) = " << info.freeram   / factor << '\n'
			  << " [*]                    Total SWAP (Gb) = " << info.totalswap / factor << '\n'				  
			  << " [*]                     Free SWAP (Gb) = " << info.freeswap  / factor << '\n'				  				  
			  << " [*]        Number of processes running = " << info.procs << '\n'
			  << '\n';
}

// Reported by command $ uptime 
void show_uptime()
{	
	auto ifs = std::ifstream("/proc/uptime");
	if( !ifs.good() ){ throw std::runtime_error("Error: unable to open uptime file "); }
	double seconds;
	ifs >> seconds; 
	uint64_t factor = 24 * 60 * 60;
	std::cout << "   Uptime in hours = " << seconds / ( 60 * 60) << '\n';
	std::cout << "    Uptime in days = " << seconds / factor << '\n';
}


void show_kernel_runtime_config()
{
	auto show_field= [](std::string const& label, std::string const& file)
	{
		std::cout << " [*] " 
				  << std::setw(40) << std::right << (label + " => ")
				  << std::setw(50) << std::left  << readFile(file)
				  << '\n';
	};

	std::cout << '\n';
	show_field("Machine Hostname",                "/proc/sys/kernel/hostname");
	show_field("Kernel Version",                  "/proc/sys/kernel/osrelease");	
	show_field("Os type",                         "/proc/sys/kernel/ostype");
	show_field("Boot unique ID identifier",       "/proc/sys/kernel/random/boot_id");
	show_field("Date which kernel was compiled",  "/proc/sys/kernel/version");
	show_field("Sysrq enabled (Emergency keys)",  "/proc/sys/kernel/sysrq");
	show_field("Max number of Cgroups namespaces", "/proc/sys/user/max_cgroup_namespaces");
	show_field("Max number of Mount namespaces",   "/proc/sys/user/max_net_namespaces");
}


void show_network_interfaces()
{
	throw std::runtime_error("Error: not implemented yet.");
}

// Show mounted file systems, but ignore 'cgroup', 'overlay', 'fusectl'   
void show_mount_points()
{
	auto ifs = std::ifstream("/proc/mounts");
	if(!ifs){ 
		throw std::runtime_error("Error: unable to open file /proc/mounts"); 
	}
	std::cout << '\n';
	std::cout << "    " 
			  << std::setw(20) << std::left << "File system" 
			  << std::setw(20) << std::left << "Type"
			  << std::setw(10) << std::left << "Mount point"
			  << '\n';
	
	std::cout << "    " 
			  << std::setw(20) << std::left << "------------" 
			  << std::setw(20) << std::left << "----"
			  << std::setw(10) << std::left << "-----------"
			  << '\n';

	std::string line, fsystem, type, mount_directory;	

	while( std::getline(ifs, line) )
	{
		auto ss = std::stringstream(line);
		ss >> fsystem >> mount_directory >> type;
		if( type != "cgroup" && type != "overlay" && type != "cgroup2")
		{
			std::cout << "   " 
					  << std::setw(20) << std::left << fsystem 
					  << std::setw(20) << std::left << type 
					  << std::setw(10) << std::left << mount_directory 
					  << '\n';
		}
	}
}

// Requires: <string>, <stream>, <sstream>
std::string readFile(std::string const& file) 
{
    std::ifstream is(file);
    if( !is.good() ){
        throw std::runtime_error("Error: stream has errors.");
    }
    std::stringstream ss;
    ss << is.rdbuf();	
	std::string m;
	// Remove ending line character '\n' or '\r\n'.
	std::getline(ss, m);
    return m;
} 
