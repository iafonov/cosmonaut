class Cosmonaut
  def rebuild
    clean
    build
  end

  def start
    @server_pid = fork do
      exec "./src/cosmonaut"
    end
  end

  def stop
    shell_out!("kill -9 #{@server_pid}")
  end

private

  def clean
    shell_out!("make clean")
  end

  def build
    shell_out!("make")
  end
end