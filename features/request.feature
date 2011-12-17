Feature: Requesting cosmonaut

Scenario: GET request
  Given cosmonaut is running
  When I run GET request
  Then I receive non-empty response
