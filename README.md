# csvtodict

Python bindings to a very lightweight CSV to Python dictionary reader.


## Getting started

```
pip install .
```

```
import csvtodict

data = csvtodict.load("my_file.csv")
```

## Features

Automatically determines if values are string, integer, or float.