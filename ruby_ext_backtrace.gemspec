# -*- encoding: utf-8 -*-
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'ruby_ext_backtrace/version'

Gem::Specification.new do |gem|
  gem.name          = "ruby_ext_backtrace"
  gem.version       = RubyExtBacktrace::VERSION
  gem.authors       = ["Vasily Fedoseyev"]
  gem.email         = ["vasilyfedoseyev@gmail.com"]
  gem.description   = %q{Extended backtrace for ruby}
  gem.summary       = %q{have you ever wanted to see method parameters in backtrace?}
  gem.homepage      = ""

  gem.files         = `git ls-files`.split($/)
  gem.executables   = gem.files.grep(%r{^bin/}).map{ |f| File.basename(f) }
  gem.test_files    = gem.files.grep(%r{^(test|spec|features)/})
  gem.require_paths = ["lib"]

  gem.extensions = "ext/ruby_ext_backtrace/extconf.rb"

  gem.add_dependency "debugger-ruby_core_source"
  gem.add_development_dependency('rake-compiler')
end
