@javascript
Feature: Requesting cosmonaut

Scenario: GET request
  When I run GET request
  Then I receive non-empty response

Scenario: Home page
  When I go to /
  Then I should see "It works!"