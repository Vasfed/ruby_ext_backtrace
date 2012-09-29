require "ruby_ext_backtrace/version"

require 'rbconfig'
require "ruby_ext_backtrace.#{RbConfig::CONFIG['DLEXT']}"

module RubyExtBacktrace
  # Everything is in extension atm
end
