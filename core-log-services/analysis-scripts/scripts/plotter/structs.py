from dataclasses import dataclass
from typing import Callable
from extractStrategies import extract_min_max_mean

@dataclass
class searchLookUp:
    type: str = ""
    searchString: str = ""
    extractFunction: Callable = extract_min_max_mean

@dataclass
class measValue:
    time: str = ""
    value: str = ""