task :default => [:build, :run]

desc "Build"
task :build do
  system('rm cosmonaut')
  system('make cosmonaut')
end

desc "Run"
task :run do
  system('./cosmonaut')
end