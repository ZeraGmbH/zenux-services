import unittest
import sys
sys.path.insert(0, '.')
from log_analysis import process_key_strings, process_limit_strings, check_strings

def get_testFile():
        return "tests/data/example-journal.log-0"

class Test_logAnalysis(unittest.TestCase):
    
    def test_check_strings(self):
        # core dump in test log in line 1205 if started counting at 0
        self.assertEqual(check_strings(get_testFile(),["Failed with result 'core-dump'"])[0], 1205)

    def test_process_key_strings(self):
        capturedOutput = process_key_strings(get_testFile())                                   
        expected_result = "Jun 24 13:25:07.409967 zera-mt310s2-unknown systemd[1]: modulemanager.service: Failed with result 'core-dump'."
        self.assertEqual(capturedOutput[3], expected_result)
         
    def test_process_key_strings_invalid_input(self):
         self.assertEqual([], process_key_strings("foo"))

    def test_process_key_strings_empty_file(self):
         open("/tmp/python_test_file.log", 'a')
         self.assertEqual([], process_key_strings("/tmp/python_test_file.log"))

    def test_process_limit_strings(self):
        capturedOutput = process_limit_strings(get_testFile(), [(" Memory used: ", 30)])
        expected_result = "Jun 24 13:25:24.905289 zera-mt310s2-unknown system-metrics-logger[365]: Memory used: 30.6%, Buffers: 1.3% Cache: 24.4%"
        self.assertEqual(capturedOutput[3], expected_result)

    def test_process_limit_strings_invalid_input(self):
        self.assertEqual([], process_limit_strings("foo"))

    def test_process_limit_strings_empty_file(self):
        open("/tmp/python_test_file.log", 'a')
        self.assertEqual([], process_limit_strings("/tmp/python_test_file.log"))
         
if __name__ == "__main__":
    unittest.main()
