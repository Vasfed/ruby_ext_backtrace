#!/usr/bin/env ruby
#encoding: utf-8

$:.unshift(File.dirname(__FILE__))

require 'pp'
require 'ruby_ext_backtrace'

def some_method2 param
  c1 = caller_ext(0); c2 = caller(0)
  pp c1
  puts "\n #Original caller for comparison:"
  pp c2
end

def some_method param1, param2=nil, *splat, &blk
  yield "block_param0", "block_param1"
end

some_method(1111, nil, 1, 2, 3){|bp1,bp2|
  some_method2 "some_method2_param"
}