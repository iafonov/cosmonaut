require 'mixlib/shellout'

def shell_out!(shell_command)
  command = Mixlib::ShellOut.new(shell_command)
  command.run_command
  command.stdout
end