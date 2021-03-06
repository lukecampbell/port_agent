#include "common/util.h"
#include "common/logger.h"
#include "common/exception.h"
#include "gtest/gtest.h"

#include <fstream>
#include <string>

using namespace std;
using namespace logger;

#define TESTFILE "/tmp/testfile.txt"

class UtilTest : public testing::Test {
    
    protected:
        virtual void SetUp() {
            Logger::SetLogFile("/tmp/gtest.log");
            Logger::SetLogLevel("DEBUG");
            
            LOG(INFO) << "************************************************";
            LOG(INFO) << "            Util Start Up";
            LOG(INFO) << "************************************************";
            remove_file(TESTFILE);
        }
    
        virtual void TearDown() {
            LOG(INFO) << "UtilTest TearDown";
            remove_file(TESTFILE);
        }
    
        ~UtilTest() {
            LOG(INFO) << "UtilTest dtor";
        }
};


/* Test byte to unsigned it */
TEST_F(UtilTest, UnsignedInt) {
    uint8_t byte = 0xC3;
    
    EXPECT_EQ(byte, byteToUnsignedInt(byte));
}

/* Test file_exists */
TEST_F(UtilTest, FileExists) {
    remove_file(TESTFILE);
    EXPECT_FALSE(file_exists(TESTFILE));
    
    ofstream outfile(TESTFILE);
    if(!outfile)
        throw TestPrereqFailed( "Failed to create test output file" );
    outfile.close();
    
    EXPECT_TRUE(file_exists(TESTFILE));
    remove_file(TESTFILE);
    
    EXPECT_FALSE(file_exists(TESTFILE));
}


/* Test if we can read a file */
TEST_F(UtilTest, ReadFile) {
    string content("this is a test\nline 2");
    string result;
    
    ofstream outfile(TESTFILE);
    if(!outfile)
        throw TestPrereqFailed( "Failed to create test output file" );
    
    outfile << content;
    outfile.close();
    
    result = read_file(TESTFILE);
    
    ASSERT_EQ(content, result);
}

/* Test if we can create a file with content */
TEST_F(UtilTest, CreateFile) {
    string content("this is a test");
    string result;
    
    create_file(TESTFILE, content.c_str());
    result = read_file(TESTFILE);
    
    EXPECT_EQ(content, result);
}


/* Test remove_file -
   positive tests happen in the FileExists test.  We can do some
   negative testing here */
TEST_F(UtilTest, RemoveFile) {
    /* Start with a file that doesn't exists, then remove it again */
    LOG(INFO) << "Ensure " << TESTFILE << "Doesn't exist";
    remove_file(TESTFILE);
    EXPECT_FALSE(file_exists(TESTFILE));
    
    LOG(INFO) << "Try to remove " << TESTFILE << " knowing it doesn't exist";
    EXPECT_TRUE(remove_file(TESTFILE));
    
    /* In order to test permissions you need to create a file in a directory
     * without write permissions. Then try to remove it.  It's kind of a PITA
     * to do programatically so uncomment this test if you want to try
     * dr-x------   3 wfrench        wheel      102 Aug  2 15:24 foo
     * $ ls -l /tmp/foo
     * total 0
     * -rw-r--r--  1 wfrench  wheel  0 Aug  2 15:24 file
     */

    //EXPECT_FALSE(remove_file("/tmp/foo/file"));
    
    
}

/* Test stack trace */
// the glibc backtrace method isn't working as I expect it to on the TS7370.
// disabling this test untill we can figure out why.
TEST_F(UtilTest, DISABLED_StackTrace) {
    string trace = stackTrace();
    LOG(DEBUG) << "Stack Trace: " << endl << trace;

    EXPECT_TRUE(trace.length());
}

/* Test chomp */
TEST_F(UtilTest, Chomp) {
    string target;
    string expected = "foo";
    
    // No newlines
    target = "foo";
    chomp(target);
    EXPECT_EQ(target, expected);
    
    // \n
    target = "foo\n";
    chomp(target);
    EXPECT_EQ(target, expected);
    
    // \r
    target = "foo\r";
    chomp(target);
    EXPECT_EQ(target, expected);
    
    // \r\n
    target = "foo\r\n";
    chomp(target);
    EXPECT_EQ(target, expected);
    
    // \n\r
    target = "foo\n\r";
    chomp(target);
    EXPECT_EQ(target, expected);
    
    // This is not trailing so it won't be chomped
    target = "foo\n\rbar";
    expected = "foo\n\rbar";
    EXPECT_EQ(target, expected);
}

