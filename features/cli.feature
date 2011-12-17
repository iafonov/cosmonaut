Feature: Command line interface

Scenario: Custom configuration file
  When I start cosmonaut and supply "test_cosmonaut.conf" as config file
  Then cosmonaut should listen port 31338
   And cosmonaut should pickup "test_cosmonaut.conf" as config file