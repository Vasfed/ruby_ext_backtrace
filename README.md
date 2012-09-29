# RubyExtBacktrace

Provides extended backtrace for ruby, including method parameters values.

## Installation

Add this line to your application's Gemfile:

    gem 'ruby_ext_backtrace'

And then execute:

    $ bundle

Or install it yourself as:

    $ gem install ruby_ext_backtrace

## Usage

Once loaded, gem provides global method caller_ext, similar to `Kernel#caller`, but returning an array of hashes.

```ruby

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
```

this outputs:

```ruby

["./test.rb:10:in `meth'",
 "./test.rb:17:in `some_method2'",
 "./test.rb:25:in `block in <main>'",
 "./test.rb:21:in `some_method'",
 "./test.rb:24:in `<main>'"]

[{:file=>"./test.rb",
  :line=>10,
  :method=>"meth",
  :argc=>1,
  0=>"default_meth_param"},
 {:file=>"./test.rb",
  :line=>17,
  :method=>"some_method2",
  :argc=>1,
  0=>"some_method2_param"},
 {:file=>"./test.rb",
  :line=>25,
  :method=>"block in <main>",
  :argc=>2,
  0=>"block_param0",
  1=>"block_param1"},
 {:file=>"./test.rb",
  :line=>21,
  :method=>"some_method",
  :argc=>3,
  0=>1111,
  1=>nil,
  2=>#<Proc:0x007f8c5b02fff0@./test.rb:24>},
 {:file=>"./test.rb", :line=>24, :method=>"<main>", :argc=>0}]
```

Note that fetching parameters for CFUNCs with variable parameters (argc&lt;0) is not supported.
Also instead of "&lt;ruby&gt;" `:file` can be `nil`, so does `:method` for unknown methods.

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create new Pull Request
