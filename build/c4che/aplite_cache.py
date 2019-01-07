AR = 'arm-none-eabi-ar'
ARFLAGS = 'rcs'
AS = 'arm-none-eabi-gcc'
BINDIR = '/usr/local/bin'
BLOCK_MESSAGE_KEYS = []
BUILD_DIR = 'aplite'
BUILD_TYPE = 'app'
BUNDLE_BIN_DIR = 'aplite'
BUNDLE_NAME = 'SyncHealthInfoNative.pbw'
CC = ['arm-none-eabi-gcc']
CCLNK_SRC_F = []
CCLNK_TGT_F = ['-o']
CC_NAME = 'gcc'
CC_SRC_F = []
CC_TGT_F = ['-c', '-o']
CC_VERSION = ('4', '7', '2')
CFLAGS = ['-std=c99', '-mcpu=cortex-m3', '-mthumb', '-ffunction-sections', '-fdata-sections', '-g', '-fPIE', '-Os', '-D_TIME_H_', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter', '-Wno-error=unused-function', '-Wno-error=unused-variable']
CFLAGS_MACBUNDLE = ['-fPIC']
CFLAGS_cshlib = ['-fPIC']
CPPPATH_ST = '-I%s'
DEFINES = ['RELEASE', 'PBL_PLATFORM_APLITE', 'PBL_BW', 'PBL_RECT', 'PBL_COMPASS', 'PBL_DISPLAY_WIDTH=144', 'PBL_DISPLAY_HEIGHT=168', 'PBL_SDK_3']
DEFINES_ST = '-D%s'
DEST_BINFMT = 'elf'
DEST_CPU = 'arm'
DEST_OS = 'darwin'
INCLUDES = ['aplite']
LD = 'arm-none-eabi-ld'
LIBDIR = '/usr/local/lib'
LIBPATH_ST = '-L%s'
LIB_DIR = 'node_modules'
LIB_ST = '-l%s'
LINKFLAGS = ['-mcpu=cortex-m3', '-mthumb', '-Wl,--gc-sections', '-Wl,--warn-common', '-fPIE', '-Os']
LINKFLAGS_MACBUNDLE = ['-bundle', '-undefined', 'dynamic_lookup']
LINKFLAGS_cshlib = ['-shared']
LINKFLAGS_cstlib = ['-Wl,-Bstatic']
LINK_CC = ['arm-none-eabi-gcc']
MESSAGE_KEYS = {u'dummy': 10000}
MESSAGE_KEYS_DEFINITION = '/Users/amirfahmideh/Projects/SyncHealthInfoNative/SyncHealthInfoNative/build/src/message_keys.auto.c'
MESSAGE_KEYS_HEADER = '/Users/amirfahmideh/Projects/SyncHealthInfoNative/SyncHealthInfoNative/build/include/message_keys.auto.h'
MESSAGE_KEYS_JSON = '/Users/amirfahmideh/Projects/SyncHealthInfoNative/SyncHealthInfoNative/build/js/message_keys.json'
NODE_PATH = '/Users/amirfahmideh/Library/Application Support/Pebble SDK/SDKs/current/node_modules'
PEBBLE_SDK_COMMON = '/Users/amirfahmideh/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble/common'
PEBBLE_SDK_PLATFORM = '/Users/amirfahmideh/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble/aplite'
PEBBLE_SDK_ROOT = '/Users/amirfahmideh/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble'
PLATFORM = {'TAGS': ['aplite', 'bw', 'rect', 'compass', '144w', '168h'], 'MAX_FONT_GLYPH_SIZE': 256, 'ADDITIONAL_TEXT_LINES_FOR_PEBBLE_H': [], 'MAX_APP_BINARY_SIZE': 65536, 'MAX_RESOURCES_SIZE': 524288, 'MAX_APP_MEMORY_SIZE': 24576, 'MAX_WORKER_MEMORY_SIZE': 10240, 'NAME': 'aplite', 'BUNDLE_BIN_DIR': 'aplite', 'BUILD_DIR': 'aplite', 'MAX_RESOURCES_SIZE_APPSTORE_2_X': 98304, 'MAX_RESOURCES_SIZE_APPSTORE': 131072, 'DEFINES': ['PBL_PLATFORM_APLITE', 'PBL_BW', 'PBL_RECT', 'PBL_COMPASS', 'PBL_DISPLAY_WIDTH=144', 'PBL_DISPLAY_HEIGHT=168']}
PLATFORM_NAME = 'aplite'
PREFIX = '/usr/local'
PROJECT_INFO = {'appKeys': {u'dummy': 10000}, u'watchapp': {u'watchface': False}, u'displayName': u'Sync Health Info', u'uuid': u'65cc2e20-8534-43cf-ad97-e62fb14219dc', u'messageKeys': {u'dummy': 10000}, 'companyName': u'Amir Fahmideh', u'enableMultiJS': True, u'sdkVersion': u'3', 'versionLabel': u'1.0', u'targetPlatforms': [u'aplite', u'basalt', u'chalk', u'diorite'], 'longName': u'Sync Health Info', 'shortName': u'Sync Health Info', u'resources': {u'media': [{u'type': u'bitmap', u'name': u'action_bar_icon_up', u'file': u'images/action_bar_icon_up.png'}, {u'type': u'bitmap', u'name': u'action_bar_icon_check', u'file': u'images/action_bar_icon_check.png'}, {u'type': u'bitmap', u'name': u'action_bar_icon_down', u'file': u'images/action_bar_icon_down.png'}]}, 'name': u'SyncHealthInfo'}
REQUESTED_PLATFORMS = [u'aplite', u'basalt', u'chalk', u'diorite']
RESOURCES_JSON = [{u'type': u'bitmap', u'name': u'action_bar_icon_up', u'file': u'images/action_bar_icon_up.png'}, {u'type': u'bitmap', u'name': u'action_bar_icon_check', u'file': u'images/action_bar_icon_check.png'}, {u'type': u'bitmap', u'name': u'action_bar_icon_down', u'file': u'images/action_bar_icon_down.png'}]
RPATH_ST = '-Wl,-rpath,%s'
SANDBOX = False
SDK_VERSION_MAJOR = 5
SDK_VERSION_MINOR = 78
SHLIB_MARKER = None
SIZE = 'arm-none-eabi-size'
SONAME_ST = '-Wl,-h,%s'
STLIBPATH_ST = '-L%s'
STLIB_MARKER = None
STLIB_ST = '-l%s'
SUPPORTED_PLATFORMS = ['basalt', 'aplite', 'diorite', 'chalk', 'emery']
TARGET_PLATFORMS = ['diorite', 'chalk', 'basalt', 'aplite']
TIMESTAMP = 1546859854
USE_GROUPS = True
VERBOSE = 0
WEBPACK = '/Users/amirfahmideh/Library/Application Support/Pebble SDK/SDKs/current/node_modules/.bin/webpack'
cprogram_PATTERN = '%s'
cshlib_PATTERN = 'lib%s.so'
cstlib_PATTERN = 'lib%s.a'
macbundle_PATTERN = '%s.bundle'
