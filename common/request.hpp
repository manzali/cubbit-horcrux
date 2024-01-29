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

        // This helper function deduces the type and assigns the value with the matching key
        template <class T>
        void extract(bj::object const &obj, T &t, std::string_view key)
        {
            t = bj::value_to<T>(obj.at(key));
        }

        struct save_request_obj
        {
            std::string request_type;
            std::string uuid;
            unsigned int total_horcruxes;
            unsigned int id_horcrux;
            std::string horcrux;
        };

        void tag_invoke(bj::value_from_tag, bj::value &jv, save_request_obj const &req)
        {
            jv = {
                {"request_type", req.request_type},
                {"uuid", req.uuid},
                {"total_horcruxes", req.total_horcruxes},
                {"id_horcrux", req.id_horcrux},
                {"horcrux", req.horcrux}};
        }

        save_request_obj tag_invoke(bj::value_to_tag<save_request_obj>, bj::value const &jv)
        {
            save_request_obj req;
            bj::object const &obj = jv.as_object();
            extract(obj, req.request_type, "request_type");
            extract(obj, req.uuid, "uuid");
            extract(obj, req.total_horcruxes, "total_horcruxes");
            extract(obj, req.id_horcrux, "id_horcrux");
            extract(obj, req.horcrux, "horcrux");
            return req;
        }

        struct load_request_obj
        {
            std::string request_type;
            std::string uuid;
        };

        struct save_reply_obj
        {
            std::string request_type;
            unsigned int status_code;
        };

        struct load_reply_obj
        {
            std::string request_type;
            unsigned int status_code;
            unsigned int total_horcruxes;
            unsigned int id_horcrux;
            std::string horcrux;
        };

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

        std::string generate_save_request(std::string const &uuid, unsigned int id_h, std::string const &horcrux)
        {
            return get_request_string_from_type(request_type::SAVE) + hd::field_delimiter +
                   uuid + hd::field_delimiter +
                   std::to_string(id_h) + hd::field_delimiter +
                   horcrux + hd::message_delimiter;
        }

        request_obj generate_load_request(std::string const &uuid)
        {
            return request_obj{
                {"request_type", get_request_string_from_type(request_type::LOAD)},
                {"uuid", uuid}};
        }

        request_obj generate_save_reply(unsigned int status_code)
        {
            return request_obj{
                {"request_type", get_request_string_from_type(request_type::SAVE)},
                {"status_code", status_code}};
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