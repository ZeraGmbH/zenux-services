from dataclasses import dataclass
from typing import Callable
from extractStrategies import extract_min_max_mean, extract_dsp, extract_dsp_read_write, extract_meas_freq, extract_min_max_mean_dsp, extract_min_max_mean_total_int

@dataclass
class searchLookUp:
    type: str = ""
    searchString: str = ""
    extractFunction: Callable = extract_min_max_mean
    label: str = ""
    label2: str = ""

@dataclass
class measValue:
    time: str = ""
    value: str = ""
    value2: str = ""
