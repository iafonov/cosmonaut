Given /^cosmonaut is running$/ do
  COSMONAUT.start
end

When /^I start cosmonaut and supply "([^"]*)" as config file$/ do |conf_file_name|
  COSMONAUT.start("--config #{fixture_file_path(conf_file_name)}")
end

Then /^cosmonaut should pickup "([^"]*)" as config file$/ do |conf_file_name|
  COSMONAUT.std_err.should include("#{fixture_file_path(conf_file_name)}")
end

Then /^cosmonaut should listen port (\d+)$/ do |port|
  shell_out!("lsof -i -P | grep -i \"cosmonaut.*listen\"").should_not be_empty
end