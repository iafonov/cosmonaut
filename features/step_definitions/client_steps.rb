When /^I run GET request$/ do
  command = Mixlib::ShellOut.new("curl 127.0.0.1:31337")
  command.run_command
  @output = command.stdout
end

Then /^I receive non\-empty response$/ do
  @output.should_not be_empty
end
