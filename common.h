#pragma once

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "xinput.lib")

// Made this for myself, disables string encryption so I can upload the .dll on UnknownCheats
//#define BUILD_FOR_UC

#include <SDKDDKVer.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include <Xinput.h>
#include <wrl/client.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <atomic>
#include <mutex>
#include <thread>

#include <memory>
#include <new>

#include <sstream>
#include <string>
#include <string_view>

#include <algorithm>
#include <functional>
#include <utility>

#include <stack>
#include <vector>

#include <typeinfo>
#include <type_traits>

#include <exception>
#include <stdexcept>

#include <any>
#include <optional>
#include <variant>

#include <regex>
#include <random>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "Utilities/xorstr.h"

#include "logger.h"