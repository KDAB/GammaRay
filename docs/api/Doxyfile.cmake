#---------------------------------------------------------------------------
# Project related configuration options
#---------------------------------------------------------------------------
PROJECT_NAME           = "GammaRay API Documentation"
PROJECT_NUMBER         = @GAMMARAY_VERSION@
OUTPUT_DIRECTORY       = "@DOXYGEN_OUTPUT_DIR@"
CREATE_SUBDIRS         = NO
OUTPUT_LANGUAGE        = English
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       = "The \$name class" \
                         "The \$name widget" \
                         "The \$name file" \
                         is \
                         provides \
                         specifies \
                         contains \
                         represents \
                         a \
                         an \
                         the
ALWAYS_DETAILED_SEC    = NO
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = NO
STRIP_FROM_PATH        =
STRIP_FROM_INC_PATH    =
SHORT_NAMES            = NO
QT_AUTOBRIEF           = YES
MULTILINE_CPP_IS_BRIEF = NO
INHERIT_DOCS           = YES
SEPARATE_MEMBER_PAGES  = NO
TAB_SIZE               = 8
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
BUILTIN_STL_SUPPORT    = NO
DISTRIBUTE_GROUP_DOC   = NO
SUBGROUPING            = YES
#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------
EXTRACT_ALL            = NO
EXTRACT_PRIVATE        = NO
EXTRACT_STATIC         = YES
EXTRACT_LOCAL_CLASSES  = NO
EXTRACT_LOCAL_METHODS  = NO
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = YES
HIDE_IN_BODY_DOCS      = YES
INTERNAL_DOCS          = YES
CASE_SENSE_NAMES       = YES
HIDE_SCOPE_NAMES       = NO
SHOW_INCLUDE_FILES     = YES
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_MEMBERS_CTORS_1ST = YES
SORT_BRIEF_DOCS        = YES
SORT_BY_SCOPE_NAME     = NO
GENERATE_TODOLIST      = NO
GENERATE_TESTLIST      = NO
GENERATE_BUGLIST       = NO
GENERATE_DEPRECATEDLIST = YES
ENABLED_SECTIONS       =
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = NO
FILE_VERSION_FILTER    =
SHOW_FILES             = NO
#---------------------------------------------------------------------------
# configuration options related to warning and progress messages
#---------------------------------------------------------------------------
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_NO_PARAMDOC       = YES
WARN_FORMAT            = "$file:$line: $text"
WARN_LOGFILE           = doxygen.log
#---------------------------------------------------------------------------
# configuration options related to the input files
#---------------------------------------------------------------------------
INPUT                  = @DOXYGEN_INPUT@
FILE_PATTERNS          = *.cpp \
                         *.cc \
                         *.cxx \
                         *.h \
                         *.hh \
                         *.hxx \
                         *.hpp \
                         *.dox
RECURSIVE              = YES
EXCLUDE                = "@CMAKE_SOURCE_DIR@/common/gammaray_common_export.h" \
                         "@CMAKE_SOURCE_DIR@/core/gammaray_core_export.h" \
                         "@CMAKE_SOURCE_DIR@/ui/gammaray_ui_export.h"
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       = */.svn/* \
                         */.git/* \
                         */cmake/* \
                         *.moc.* \
                         moc* \
                         *.all_cpp.* \
                         *unload.* \
                         */test/* \
                         */tests/* \
                         *_p.cpp \
                         *_export.h
EXAMPLE_PATH           =
EXAMPLE_PATTERNS       = *
EXAMPLE_RECURSIVE      = NO
IMAGE_PATH             = "@CMAKE_CURRENT_SOURCE_DIR@"
INPUT_FILTER           =
FILTER_PATTERNS        =
FILTER_SOURCE_FILES    = NO
#---------------------------------------------------------------------------
# configuration options related to the alphabetical class index
#---------------------------------------------------------------------------
ALPHABETICAL_INDEX     = NO
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          =
#---------------------------------------------------------------------------
# do NOT generate any formats other than html
#---------------------------------------------------------------------------
SOURCE_BROWSER         = NO
GENERATE_HTML          = YES
GENERATE_HTMLHELP      = NO
GENERATE_LATEX         = NO
GENERATE_RTF           = NO
GENERATE_XML           = NO
GENERATE_AUTOGEN_DEF   = NO
GENERATE_PERLMOD       = NO
#---------------------------------------------------------------------------
# configuration options related to the HTML output
#---------------------------------------------------------------------------
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            =
HTML_FOOTER            = "@CMAKE_CURRENT_SOURCE_DIR@/footer.html"
HTML_EXTRA_FILES       = "@CMAKE_CURRENT_SOURCE_DIR@/kdab-logo-16x16.png" "@CMAKE_CURRENT_SOURCE_DIR@/kdab-gammaray-logo-16x16.png"
DISABLE_INDEX          = NO
ENUM_VALUES_PER_LINE   = 4
GENERATE_TREEVIEW      = NO
VERBATIM_HEADERS       = NO
#---------------------------------------------------------------------------
# Configuration options related to the preprocessor
#---------------------------------------------------------------------------
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = YES
EXPAND_ONLY_PREDEF     = NO
SEARCH_INCLUDES        = YES
INCLUDE_PATH           =
INCLUDE_FILE_PATTERNS  =
PREDEFINED             = Q_DECL_IMPORT="" GAMMARAY_COMMON_EXPORT="" GAMMARAY_CORE_EXPORT="" \
                         GAMMARAY_UI_EXPORT="" GAMMARAY_LAUNCHER_EXPORT="" \
                         GAMMARAY_LAUNCHER_DEPRECATED_EXPORT="" \
                         override="override"
EXPAND_AS_DEFINED      =
SKIP_FUNCTION_MACROS   = YES
#---------------------------------------------------------------------------
# Configuration::additions related to external references
#---------------------------------------------------------------------------
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
PERL_PATH              = /usr/bin/perl
#---------------------------------------------------------------------------
# Configuration options related to the dot tool
#---------------------------------------------------------------------------
CLASS_DIAGRAMS         = YES
HIDE_UNDOC_RELATIONS   = YES
HAVE_DOT               = @HAVE_DOT@
CLASS_GRAPH            = YES
COLLABORATION_GRAPH    = NO
GROUP_GRAPHS           = NO
UML_LOOK               = NO
TEMPLATE_RELATIONS     = NO
INCLUDE_GRAPH          = NO
INCLUDED_BY_GRAPH      = NO
CALL_GRAPH             = NO
CALLER_GRAPH           = NO
GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = NO
DOT_IMAGE_FORMAT       = png
DOT_PATH               =
DOTFILE_DIRS           =
MAX_DOT_GRAPH_DEPTH    = 1000
DOT_TRANSPARENT        = NO
DOT_MULTI_TARGETS      = NO
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
#---------------------------------------------------------------------------
# Configuration::additions related to the search engine
#---------------------------------------------------------------------------
SEARCHENGINE           = NO
SERVER_BASED_SEARCH    = NO
#---------------------------------------------------------------------------
# configuration options related to the qhp output
#---------------------------------------------------------------------------
GENERATE_QHP           = YES
QCH_FILE               = @DOXYGEN_OUTPUT_DIR@/qch/gammaray-api.qch
QHP_NAMESPACE          = com.kdab.GammaRay.api.@GAMMARAY_PLUGIN_VERSION@
QHP_VIRTUAL_FOLDER     = GammaRay-@GAMMARAY_VERSION@
QHG_LOCATION           = @QHELPGEN_EXECUTABLE@

# Cross-references to Qt documentation
TAGFILES = @QDOC_TAG_DIR@/qtcore/qtcore.tags=qthelp://org.qt-project.qtcore/qtcore/ \
           @QDOC_TAG_DIR@/qtgui/qtgui.tags=qthelp://org.qt-project.qtgui/qtgui/ \
           @QDOC_TAG_DIR@/qtwidgets/qtwidgets.tags=qthelp://org.qt-project.qtwidgets/qtwidgets/
