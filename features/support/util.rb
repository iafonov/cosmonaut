def fixture_file_path(file_name)
  File.join("features", "fixture_files", file_name)
end

def uploads_dir
  File.join("features", "test_app", "public", "uploads")
end