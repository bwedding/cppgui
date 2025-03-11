#pragma once
#include <plog/Record.h>
#include <plog/Util.h>
#include <iomanip>
#include <chrono>
#include <format>
#include <iostream>
using namespace std::chrono;

namespace plog
{
    template<bool useUtcTime>
    class MSVCDebugFormatter
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        {
            const auto time_pt_utc = system_clock::from_time_t(record.getTime().time) + milliseconds(record.getTime().millitm);

            util::nostringstream ss;
            if constexpr (useUtcTime)  // Use UTC time zone
                ss << time_point_cast<milliseconds>(time_pt_utc);
            else // Use local time zone 
               ss << time_point_cast<milliseconds>(current_zone()->to_local(time_pt_utc));

            ss  << PLOG_NSTR(" ")  << record.getFunc() << PLOG_NSTR(":") << record.getLine() << PLOG_NSTR(" - ") << record.getMessage() << PLOG_NSTR("\n");
            return ss.str();
        }
    };
}

