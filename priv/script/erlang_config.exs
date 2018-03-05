defmodule ErlangConfig do
  def main(["root"]) do
    IO.puts root()
  end

  def main(["ei"]) do
    path = :code.lib_dir('erl_interface')
    IO.puts "#{path}"
  end

  def main(["erts"]) do
    root = :code.root_dir()
    libdir = :code.lib_dir('erts')
    [vsn| _] = Regex.run(~r/([0-9\.]+)$/, to_string(libdir))
    IO.puts "#{root}/erts-#{vsn}"
  end

  def main(args) do
    IO.puts "Unsupported arguments: #{args}"
  end

  defp root do
    :code.root_dir()
  end
end

ErlangConfig.main(System.argv)
