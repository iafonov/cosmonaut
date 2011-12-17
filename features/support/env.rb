require 'features/support/shell_out.rb'
require 'features/support/cosmonaut.rb'

cosmonaut = Cosmonaut.new
cosmonaut.rebuild

Before do
  cosmonaut.start
end

After do
  cosmonaut.stop
end