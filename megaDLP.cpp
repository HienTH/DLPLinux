#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void enableUSBDevice(); 
void disableUSBDevice(); 
void showAgentInfor();

int main(int argc, char** argv) {

	if(argc < 2){
		std::cerr << " [ERROR] Usage: $ " << argv[0] << " <COMMAND> " << '\n';
		return EXIT_FAILURE;
	}

	auto command = std::string{ argv[1] };

	printf("MegaDLP Endpoint For Linux !!!\n");
	printf("Ghi Nhat ky truy cap USB: ./Log/logUSBDevice.txt\n");
	printf("mega-infor: Hien thi thong tin may tinh.\n");
	printf("./megaDLP -h Huong dan su dung.\n");

	system("./monitor_usb &");

	if(command == "-h") {
		std::cout << " ./megaDLP \n";
		printf("	-e: Cho phep USB Device.\n");
		printf("	-d: Chan USB Device.\n");
	}

	if(command == "-e"){

		std::cout << "===== Cho phep USB Device ====================================" << '\n';
		enableUSBDevice();

	}

 	if(command == "-d")
	{
		std::cout << "===== Chan USB Device ====================================" << '\n';
		disableUSBDevice();

	}

	if(command == "-i")
	{
		std::cout << "===== Thong tin Agent ====================================" << '\n';
		showAgentInfor();
	}
}

void showAgentInfor(){

	std::ifstream f("agentInfor.json");
	json data = json::parse(f);
	std::cout << "Username: "<< data["username"] << '\n';
	std::cout << "Email: "<< data["email"] << '\n';
}

void disableUSBDevice() {
	system("modprobe -r usb_storage");
	system("modprobe -r uas");
	try
	{
	system("mv /lib/modules/`uname -r`/kernel/drivers/usb/storage/usb-storage.ko /lib/modules/`uname -r`/kernel/drivers/usb/storage/usb-storage.ko.blacklist");
	}
	catch (std::exception& ex)
	{
		//...
	}
}
void enableUSBDevice() {

	try
	{
		system("mv /lib/modules/`uname -r`/kernel/drivers/usb/storage/usb-storage.ko.blacklist /lib/modules/`uname -r`/kernel/drivers/usb/storage/usb-storage.ko");
	}
	catch (std::exception& e)
	{
		//...
	}

}
