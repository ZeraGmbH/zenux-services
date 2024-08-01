#!/usr/bin/env python3

import unittest
import sys
sys.path.insert(0, '../scripts/plotter/')
from plotter import parse_log_output, main
from extractStrategies import extract_min_max_mean, extract_meas_freq

#####################################################################
#                                                                   #
#   IMPORTANT NOTE                                                  #
# By Default these tests will not show up in Qt Creator             #
# Either enable them in project -> testing -> check CTest checkbox  #
# or run them locally by "./test_logAnalysis.py"                    #
#                                                                   #
#####################################################################

class Test_plotter(unittest.TestCase):
    def test_invalidLogFilePath(self):
        self.assertFalse(parse_log_output("foo", "bar"))

    def test_validLogFilePath(self):
        self.assertTrue(parse_log_output("data/plotter-journal.log-0", "cpu_temp"))

    def test_validInputToExtractStrat(self):
        testLine = "Jul 16 23:40:27.921855 zera-mt310s2-unknown system-metrics-logger[352]: CPU Temperature (°C) min: 69.2, max: 70.3, mean: 69.9"
        returnValue = extract_min_max_mean(testLine)
        self.assertEqual(returnValue.time, "Jul 16 23:40:27")
        self.assertEqual(returnValue.value, "69.9")

    def test_invalidInputToExtractStrat(self):
        testLine = "fooBarfooBarfooBar"
        returnValue = extract_min_max_mean(testLine)
        self.assertEqual(returnValue.time, "")
        self.assertEqual(returnValue.value, "")

    def test_validMeasFreqStrategy(self):
        testLine = "Jul 30 14:36:02.297406 zera-mt310s2-050082224 zera-modulemanager[279]: Measured frequency: ~13Hz"
        returnValue = extract_meas_freq(testLine)
        self.assertEqual(returnValue.time, "Jul 30 14:36:02")
        self.assertEqual(returnValue.value, "13")

if __name__ == "__main__":
    unittest.main()
