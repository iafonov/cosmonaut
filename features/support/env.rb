require 'features/support/shell_out.rb'
require 'features/support/cosmonaut.rb'
require 'features/support/util.rb'

COSMONAUT = Cosmonaut.new
COSMONAUT.rebuild
COSMONAUT.start("--config ./features/test_app/test_app.conf")

at_exit do
  COSMONAUT.stop
end