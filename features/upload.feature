Feature: File upload

Scenario: Uploading a file which size is less than chunk size
  When I upload "small_file.txt"
  Then it should be placed into uploads directory
   And its md5 checksum should match "8fa7937282b678b4b72fe6e91f26a021"

Scenario: Uploading a file which size is bigger than chunk size
  When I upload "bigger_file.txt"
  Then it should be placed into uploads directory
   And its md5 checksum should match "6ea9aab78c42ab75ebd97c6d1fcd6eb5"

Scenario: Uploading a binary file
  When I upload "cosmonaut.png"
  Then it should be placed into uploads directory
   And its md5 checksum should match "352a7c44757aa415584878a38e561b86"
