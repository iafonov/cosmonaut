require 'features/support/shell_out.rb'
require 'features/support/cosmonaut.rb'

COSMONAUT = Cosmonaut.new
COSMONAUT.rebuild

After do
  COSMONAUT.stop
end