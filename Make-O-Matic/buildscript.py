#!/usr/bin/env python

from core.Configuration import Configuration
from core.environments.Environments import Environments
from core.helpers.BoilerPlate import BuildProject
from core.helpers.PathResolver import PathResolver
from core.plugins.Preprocessor import Preprocessor
from core.plugins.builders.generators.CMakeBuilder import CMakeBuilder, CMakeVariable
from core.plugins.packagers.CPack import CPack
from core.plugins.testers.CTest import CTest
from core.plugins.reporters.EmailReporter import EmailReporter
from core.helpers.GlobalMApp import mApp
from core.plugins.platforms.BlackLister import BlackLister

build, project = BuildProject( name = 'GammaRay', version = '0.1.0', url = 'gitolite@git.kdab.com:gammaray.git' )
build.addPlugin( BlackLister( variable = 'QMAKESPEC', pattern = 'win32-g\+\+' ) )

sharedDebug = Environments( [ 'Qt-4.7.?-Shared-Debug' ], 'Qt 4 Shared Debug', project )
#sharedDebug = Environments( [], 'Qt 4 Shared Debug', project )
sharedDebug.setOptional( True )
debug = Configuration( 'Debug', sharedDebug, )
cmakeDebug = CMakeBuilder()
cmakeDebug.addCMakeVariable( CMakeVariable( 'CMAKE_BUILD_TYPE', 'debug', 'STRING' ) )
debug.addPlugin( cmakeDebug )
debug.addPlugin( CTest() )

sharedRelease = Environments( [ 'Qt-4.7.?' ], 'Qt 4 Shared Release', project )
#sharedRelease = Environments( [], 'Qt 4 Shared Release', project )
release = Configuration( 'Release', sharedRelease )
cmakeRelease = CMakeBuilder()
cmakeRelease.addCMakeVariable( CMakeVariable( 'CMAKE_BUILD_TYPE', 'release', 'STRING' ) )

release.addPlugin( cmakeRelease )
release.addPlugin( CTest() )
release.addPlugin( CPack() )

# add external contributor
#application = mApp()
#er = EmailReporter()
#application.addPlugin( er )
#er.setRecipients( ["bla@blub.org"] )
#er.setEnableFullReport( True )

build.build()
