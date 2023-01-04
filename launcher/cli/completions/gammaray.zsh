#compdef gammaray

# This file is part of GammaRay, the Qt application inspection and manipulation tool.
#
# SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
# Author: ivan tkachenko <me@ratijas.tk>
#
# SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay
#
# Licensees holding valid commercial KDAB GammaRay licenses may use this file in
# accordance with GammaRay Commercial License Agreement provided with the Software.
#
# Contact info@kdab.com if any conditions of this licensing are not clear to you.

function _gammaray-probe() {
  local -a probes names descriptions

  probes=( ${(f)"$(_call_program gammaray-probes $service --list-probes)"} )
  names=( ${probes%% *} )
  descriptions=( ${${probes#* \(}%\)} )

  _describe -t probe 'probe' \
    descriptions names
}

function _gammaray-injector() {
  local -a injectors=( gdb lldb style preload windll )
  local expl

  _wanted injector expl 'injector' \
    compadd -- $injectors
}

function _gammaray-listen() {
  local -a addresses=( 'tcp://0.0.0.0' 'tcp://[::]' )
  local expl

  if (( $+commands[ip] && $+commands[jq] )); then
    addresses+=( ${(f)"$(
      ip -j a |
      jq --raw-output '
        .[].addr_info[] |
        if .family == "inet"
          then "tcp://\(.local)"
        elif .family == "inet6"
          then "tcp://[\(.local)]"
        else null end
      '
    )"} )
  else
    addresses+=( 'tcp://127.0.0.1' )
  fi

  # sanitize special characters
  addresses=( ${addresses//:/\:} )

  _wanted address expl 'address' \
    compadd -- $addresses
}

function _gammaray-host-port() {
  if compset -P '*://*:' ; then
    _numbers -t port port
  elif compset -P '*://' ; then
    _hosts -qS:
  else
    compadd -S "" 'tcp://'
  fi
}

_arguments \
  '(* -)--list-probes[list all installed probes]' \
  '--probe[specify which probe to use]:abi:_gammaray-probe' \
  '(-i --injector)'{-i,--injector}'[set injection type]:injector:_gammaray-injector' \
  '--self-test[run self tests, of everything or the specified injector]::injector:_gammaray-injector' \
  '(-o --injector-override)'{-o,--injector-override}'[Override the injector executable if handled (requires -i/--injector)]:executable: _command_names -e' \
  "--inject-only[only inject the probe, don't show the UI]" \
  '(--inprocess --listen)--inprocess[use in-process UI]' \
  '(--inprocess --listen --no-listen)--listen[specify the address the server should listen on]:address:_gammaray-listen' \
  '(--inprocess --listen --no-listen)--no-listen[disables remote access entirely (implies --inprocess)]' \
  - '(H)' \
    '(* -)'{-h,--help}'[print program help and exit]' \
    '(* -)'{-v,--version}'[print program version and exit]' \
  - pid \
    '--pid[attach to running Qt application]:pid:_pids' \
  - connect \
    '--connect[connect to an already injected target]:host[\:port]:_gammaray-host-port' \
  - application \
    ':program: _command_names -e' \
    '*::program arguments: _normal'
