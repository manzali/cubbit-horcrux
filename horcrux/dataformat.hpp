#pragma once

#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>

namespace horcrux
{

    namespace dataformat
    {

        std::string const message_delimiter = "\r\n\r\n";

        enum status_code
        {
            SAVE_REQUEST_OK,
            SAVE_REQUEST_FAILED,
            LOAD_REQUEST_OK,
            LOAD_REQUEST_FAILED,
            UNKNOWN_REQUEST,
            ERROR_SAVING_FILE,
            ERROR_GENERATING_HORCRUXES,
            ERROR_LOADING_HORCRUXES,
            ERROR_SAVING_HORCRUX,
            HORCRUXES_FOUND,
            UNKNOWN_ERROR
        };

    }

}
