require 'features/support/shell_out.rb'
require 'features/support/cosmonaut.rb'
require 'features/support/util.rb'

require 'selenium-webdriver'
require 'capybara/cucumber'

Capybara.default_selector = :css
Capybara.javascript_driver = :selenium
Capybara.app_host = 'http://127.0.0.1:31337'
Capybara.run_server = false

COSMONAUT = Cosmonaut.new
COSMONAUT.rebuild
COSMONAUT.start("--config ./features/test_app/test_app.conf")

at_exit do
  COSMONAUT.stop
end