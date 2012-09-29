#!/usr/bin/env ruby
#encoding: utf-8

require 'mkmf'
require 'debugger/ruby_core_source'

hdrs = proc {
  have_header("vm_core.h") # and have_header("iseq.h")
}

dir_config("ruby") # allow user to pass in non-standard core include directory

if !Debugger::RubyCoreSource::create_makefile_with_core(hdrs, "ruby_ext_backtrace")
  exit(1)
end
