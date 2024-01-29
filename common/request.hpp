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
        typedef bj::value request_obj;

        enum request_type
        {
            SAVE,
            LOAD,
            UNKNOWN
        };

        request_type get_request_type_from_string(std::string const &str)
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

        std::string get_request_string_from_type(request_type r)
        {
            switch (r)
            {
            case request_type::SAVE:
                return "save";

            case request_type::LOAD:
                return "load";
            }

            return "unknown";
        }

        request_type get_request_type_from_object(request_obj const &obj)
        {
            return get_request_type_from_string(bj::value_to<std::string>(obj.as_object().at("request_type")));
        }

        bool parse_request(std::string const &str, request_obj &obj)
        {
            try
            {
                obj = bj::parse(str);
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        std::string generate_save_request(std::string const &uuid, unsigned int index, unsigned int total, std::string const &horcrux)
        {
            return bj::serialize(bj::value{
                {"request_type", get_request_string_from_type(request_type::SAVE)},
                {"uuid", uuid},
                {"index", index},
                {"total", total},
                {"horcrux", horcrux}});
        }

        bool parse_save_request(request_obj const &obj, std::string &uuid, unsigned int &index, unsigned int &total, std::string &horcrux)
        {
            try
            {
                uuid = bj::value_to<std::string>(obj.as_object().at("uuid"));
                index = bj::value_to<unsigned int>(obj.as_object().at("index"));
                total = bj::value_to<unsigned int>(obj.as_object().at("total"));
                horcrux = bj::value_to<std::string>(obj.as_object().at("horcrux"));
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        request_obj generate_load_request(std::string const &uuid)
        {
            return request_obj{
                {"request_type", get_request_string_from_type(request_type::LOAD)},
                {"uuid", uuid}};
        }

        std::string generate_save_reply(int status_code)
        {
            return bj::serialize(bj::value{
                {"request_type", get_request_string_from_type(request_type::SAVE)},
                {"status_code", status_code}});
        }

        bool parse_save_reply(request_obj const &obj, int &status_code)
        {
            try
            {
                status_code = bj::value_to<int>(obj.as_object().at("status_code"));
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        request_obj generate_load_reply(unsigned int status_code, unsigned int tot_h, unsigned int id_h, std::string const &horcrux)
        {
            return request_obj{
                {"request_type", get_request_string_from_type(request_type::LOAD)},
                {"status_code", status_code},
                {"total_horcruxes", tot_h},
                {"id_horcrux", id_h},
                {"horcrux", horcrux}};
        }

        request_obj generate_load_reply(unsigned int status_code)
        {
            // In case of missing horcrux send only status_code
            return request_obj{
                {"request_type", get_request_string_from_type(request_type::LOAD)},
                {"status_code", status_code}};
        }
        /*
                request_type parse_request(std::string str, request_obj &obj)
                {
                    try
                    {
                        obj = bj::parse(str);
                        std::string str = bj::value_to<std::string>(obj.as_object().at("request_type"));
                        return get_request_type_from_string(str);
                    }
                    catch (...)
                    {
                        return request_type::UNKNOWN;
                    }
                }
        */
        request_type parse_request_type(std::string const &data)
        {
            try
            {
                // request type id the first field of data
                std::string const str = data.substr(0, data.find(hd::field_delimiter));
                return get_request_type_from_string(str);
            }
            catch (...)
            {
                return request_type::UNKNOWN;
            }
        }

        std::string serialize_request(request_obj const &obj)
        {
            return bj::serialize(obj);
        }
    }

}