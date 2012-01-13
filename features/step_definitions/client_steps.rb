When /^I request "([^"]*)"$/ do |path|
  begin
    @response = RestClient.get("http://127.0.0.1:31337#{path}")
  rescue Exception => e
    @client_exception = e
  end
end

Then /^I should get "([^"]*)"$/ do |body|
  response_body = @client_exception.nil? ? @response.to_s : @client_exception.http_body
  response_body.should == body
end

Then /^I should get request with status "([^"]*)"$/ do |code|
  response_code = @client_exception.nil? ? @response.code : @client_exception.http_code
  response_code.should == code.to_i
end

Then /^its content type should be set to "([^"]*)"$/ do |content_type|
  @response.headers[:content_type].should == content_type
end
