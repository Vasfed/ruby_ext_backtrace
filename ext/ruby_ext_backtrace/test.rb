#!/usr/bin/env ruby
#encoding: utf-8

$:.unshift(File.dirname(__FILE__))

require 'pp'
require 'ruby_ext_backtrace'

def some_method2 param
  pp caller(0); puts; pp caller_ext(0)
end

def some_method param1, param2=nil, &blk
  yield "block_param0", "block_param1"
end

some_method(1111){|bp1,bp2|
  some_method2 "some_method2_param"
}
