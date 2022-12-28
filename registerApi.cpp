//
//  HTTPRequest
//

#include <iostream>
#include <fstream>
#include "HTTPRequest.hpp"
//#include <nlohmann/json.hpp>
//using json = nlohmann::json;

int main(){

try
{

    http::Request request{"http://192.168.1.200:9090/api/auth/register"};
    const std::string body = "{\"username\": \"test1\", \"email\": \"test1@gmail.com\"}";
    const auto response = request.send("POST", body, {
        {"Content-Type", "application/json"}
    });

    std::cout << std::string{response.body.begin(), response.body.end()} << '\n'; // print the result

    //json agentInfor = json::parse(std::string{response.body.begin(), response.body.end()});
    //std::cout << agentInfor << std::endl;

    // write prettified JSON to another file
    //std::ofstream o("agent.json");
    //o << std::setw(4) << agentInfor << std::endl;

}
catch (const std::exception& e)
{
    std::cerr << "Request failed, error: " << e.what() << '\n';
}

}
