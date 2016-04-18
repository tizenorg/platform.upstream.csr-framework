/*
 *  Boost Software License - Version 1.0 - August 17th, 2003
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 *  SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 *  FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 *  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */
#include "colour_log_formatter.h"

#include <iostream>
#include <string>

#include <boost/test/unit_test_suite_impl.hpp>
#include <boost/test/framework.hpp>
#include <boost/test/utils/basic_cstring/io.hpp>
#include <boost/test/utils/lazy_ostream.hpp>
#include <boost/version.hpp>

// ************************************************************************** //
// **************            colour_log_formatter            ************** //
// ************************************************************************** //

using namespace boost::unit_test;
namespace Csr {
namespace Test {

namespace {

const char* GREEN_BEGIN = "\033[0;32m";
const char* RED_BEGIN = "\033[0;31m";
const char* CYAN_BEGIN = "\033[0;36m";
const char* BOLD_YELLOW_BEGIN = "\033[1;33m";
const char* COLOR_END = "\033[m";

const_string
test_phase_identifier()
{
	return framework::is_initialized()
			? const_string(framework::current_test_case().p_name.get())
			: BOOST_TEST_L("Test setup");
}

const_string
get_basename(const const_string &file_name) {
	return basename(file_name.begin());
}

std::string
get_basename(const std::string &file_name) {
	return basename(file_name.c_str());
}

} // local namespace

//____________________________________________________________________________//

void
colour_log_formatter::log_start(
		std::ostream& output,
		counter_t test_cases_amount)
{
	if (test_cases_amount > 0)
		output  << "Running " << test_cases_amount << " test "
				<< (test_cases_amount > 1 ? "cases" : "case") << "..." << std::endl;
}

//____________________________________________________________________________//

void
colour_log_formatter::log_finish( std::ostream& ostr )
{
	ostr.flush();
}

//____________________________________________________________________________//

void
colour_log_formatter::log_build_info( std::ostream& output)
{
	output << "Platform: " << BOOST_PLATFORM << std::endl
			<< "Compiler: " << BOOST_COMPILER << std::endl
			<< "STL	 : " << BOOST_STDLIB << std::endl
			<< "Boost   : " << BOOST_VERSION/100000 << "."
							<< BOOST_VERSION/100 % 1000 << "."
							<< BOOST_VERSION % 100 << std::endl;
}

//____________________________________________________________________________//

void
colour_log_formatter::test_unit_start(
		std::ostream& output,
		test_unit const& tu)
{
	if (tu.p_type_name->find(const_string("suite")) == 0)
		output << "Starting test " << tu.p_type_name << " \"" << tu.p_name << "\"" << std::endl;
	else
		output << "Running test " << tu.p_type_name << " \"" << tu.p_name << "\"" << std::endl;
}

//____________________________________________________________________________//

void
colour_log_formatter::test_unit_finish(
		std::ostream& output,
		test_unit const& tu,
		unsigned long elapsed)
{
	if (tu.p_type_name->find(const_string("suite")) == 0) {
		output << "Finished test " << tu.p_type_name << " \"" << tu.p_name << "\""<< std::endl;
		return;
	}

	std::string color = GREEN_BEGIN;
	std::string status = "OK";

	if (m_isTestCaseFailed) {
		color = RED_BEGIN;
		status = "FAIL";
	}

	output << "\t" << "[   " << color << status << COLOR_END << "   ]";

	output << ", " << CYAN_BEGIN << "time: ";
	if (elapsed > 0) {
		if( elapsed % 1000 == 0 )
			output << elapsed/1000 << "ms";
		else
			output << elapsed << "mks";
	} else {
		output << "N/A";
	}

	output << COLOR_END << std::endl;
	m_isTestCaseFailed = false;
}

//____________________________________________________________________________//

void
colour_log_formatter::test_unit_skipped(
		std::ostream& output,
		test_unit const& tu)
{
	output  << "Test " << tu.p_type_name << " \"" << tu.p_name << "\"" << "is skipped" << std::endl;
}

//____________________________________________________________________________//

void
colour_log_formatter::log_exception(
		std::ostream& output,
		log_checkpoint_data const& checkpoint_data,
		boost::execution_exception const& ex)
{
	boost::execution_exception::location const& loc = ex.where();
	output << '\t' << BOLD_YELLOW_BEGIN << get_basename(loc.m_file_name)
			<< '(' << loc.m_line_num << "), ";

	output << "fatal error in \""
			<< (loc.m_function.is_empty() ? test_phase_identifier() : loc.m_function ) << "\": ";

	output << COLOR_END << ex.what();

	if (!checkpoint_data.m_file_name.is_empty()) {
		output << '\n';
		output << "\tlast checkpoint : " << get_basename(checkpoint_data.m_file_name)
				<< '(' << checkpoint_data.m_line_num << ")";
		if (!checkpoint_data.m_message.empty())
			output << ": " << checkpoint_data.m_message;
	}

	output << std::endl;
	m_isTestCaseFailed = true;
}

//____________________________________________________________________________//

void
colour_log_formatter::log_entry_start(
		std::ostream& output,
		log_entry_data const& entry_data,
		log_entry_types let)
{
	switch (let) {
	case BOOST_UTL_ET_INFO:
		output << '\t' << entry_data.m_file_name << '(' << entry_data.m_line_num << "), ";
		output << "info: ";
		break;
	case BOOST_UTL_ET_MESSAGE:
		break;
	case BOOST_UTL_ET_WARNING:
		output << '\t' << get_basename(entry_data.m_file_name) << '(' << entry_data.m_line_num << "), ";
		output << "warning in \"" << test_phase_identifier() << "\": ";
		break;
	case BOOST_UTL_ET_ERROR:
		output << '\t' << BOLD_YELLOW_BEGIN <<  get_basename(entry_data.m_file_name)
			<< '(' << entry_data.m_line_num << "), ";
		output << "error in \"" << test_phase_identifier() << "\": ";
		m_isTestCaseFailed = true;
		break;
	case BOOST_UTL_ET_FATAL_ERROR:
		output << '\t' << BOLD_YELLOW_BEGIN <<  get_basename(entry_data.m_file_name)
			<< '(' << entry_data.m_line_num << "),  ";
		output <<  " fatal error in \"" << test_phase_identifier() << "\": ";
		m_isTestCaseFailed = true;
		break;
	}
	output << COLOR_END;
}

//____________________________________________________________________________//

void
colour_log_formatter::log_entry_value(
		std::ostream& output,
		const_string value)
{
	output << value;
}

//____________________________________________________________________________//

void
colour_log_formatter::log_entry_value(
		std::ostream& output,
		lazy_ostream const& value)
{
	output << value;
}

//____________________________________________________________________________//

void
colour_log_formatter::log_entry_finish(
		std::ostream& output)
{
	output << std::endl;
}

//____________________________________________________________________________//

//____________________________________________________________________________//
} // namespace Test
} // namespace Csr

//____________________________________________________________________________//

