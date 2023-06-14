# Custom datatype mappings
# Maps datatypes unknown to ddl2cpp to known types
# This file can be copied, updated and then passed to ddl2cpp as a command line argument
# Each variable in this file must be a list of case insensitive type names.
# Usage:
#    ddl2cpp [other optional args] --datatype-file=/path/to/datatypes.py <path to ddl> <path to target> <namespace>

extendedDdlBooleanTypes = []
extendedDdlIntegerTypes = []
extendedDdlSerialTypes = []
extendedDdlFloatingPointTypes = []
extendedDdlTextTypes = ["UUID", "other_type"]
extendedDdlBlobTypes = []
extendedDdlDateTypes = []
extendedDdlDateTimeTypes = []
extendedDdlTimeTypes = []