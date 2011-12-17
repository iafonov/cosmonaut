class Cosmonaut
  def rebuild
    clean
    build
  end

  def start(cli_args = "")
    @stdout_filename = "/tmp/stdout.cosmonaut.test_run.#{$$}.log"
    @stderr_filename = "/tmp/stderr.cosmonaut.test_run.#{$$}.log"

    @server_pid = fork do
      STDOUT.reopen(File.open(@stdout_filename, "w"))
      STDERR.reopen(File.open(@stderr_filename, "w"))

      exec "./src/cosmonaut #{cli_args}"
    end

    sleep 1 while COSMONAUT.std_err.empty?
  end

  def stop
    Process.kill("TERM", @server_pid)
  end

  def std_out
    File.exists?(@stdout_filename) ? IO.read(@stdout_filename) : ""
  end

  def std_err
    File.exists?(@stderr_filename) ? IO.read(@stderr_filename) : ""
  end

private

  def clean
    shell_out!("make clean")
  end

  def build
    shell_out!("make")
  end
end