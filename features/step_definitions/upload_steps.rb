require 'rest_client'

When /^I upload "([^"]*)"$/ do |file_name|
  RestClient.post('http://127.0.0.1:31337/upload_file', :file => File.new(fixture_file_path(file_name)))

  @uploaded_file_path = Dir["#{uploads_dir}/*/#{file_name}"].first
end

Then /^it should be placed into uploads directory$/ do
  @uploaded_file_path.should_not be_nil
end

Then /^its md5 checksum should match "([^"]*)"$/ do |md5_checksum|
  Digest::MD5.file(@uploaded_file_path).to_s.should == md5_checksum
end
