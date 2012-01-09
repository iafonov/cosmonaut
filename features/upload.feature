Feature: File upload

# Scenario: Uploading a file which size is less than chunk size
#   When I upload "small_file.txt"
#   Then it should be placed into uploads directory
#    And its md5 checksum should match "8fa7937282b678b4b72fe6e91f26a021"

Scenario: Uploading a file which size is bigger than chunk size
  When I upload "bigger_file.txt"
  Then it should be placed into uploads directory
   And its md5 checksum should match "8f275224176e1e5669b54297e1cae379"
