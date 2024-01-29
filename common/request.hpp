#pragma once

#include <string>

#include <boost/json.hpp>

#include "dataformat.hpp"

namespace bj = boost::json;
namespace hd = horcrux::dataformat;

namespace horcrux
{

    namespace request
    {
        enum request_type
        {
            SAVE,
            LOAD,
            UNKNOWN
        };

        request_type get_request_type_from_string(std::string str)
        {
            if (boost::iequals(str, "save"))
            {
                return request_type::SAVE;
            }

            if (boost::iequals(str, "load"))
            {
                return request_type::LOAD;
            }

            return request_type::UNKNOWN;
        }

        bj::object generate_save_request(std::string const &uuid, unsigned int tot_h, unsigned int id_h, std::string const &horcrux)
        {
            return bj::object{
                {"uuid", uuid},
                {"total_horcruxes", tot_h},
                {"id_horcrux", id_h},
                {"horcrux", horcrux}};
        }

        bj::object generate_load_request(std::string const &uuid)
        {
            return bj::object{
                {"uuid", uuid}};
        }

        bj::object generate_save_reply(unsigned int status_code)
        {
            return bj::object{
                {"status_code", status_code}};
        }

        bj::object generate_load_reply(unsigned int status_code, unsigned int tot_h, unsigned int id_h, std::string const &horcrux)
        {
            return bj::object{
                {"status_code", status_code},
                {"total_horcruxes", tot_h},
                {"id_horcrux", id_h},
                {"horcrux", horcrux}};
        }

        bj::object generate_load_reply(unsigned int status_code)
        {
            // In case of missing horcrux send only status_code
            return bj::object{
                {"status_code", status_code}};
        }

    }

}