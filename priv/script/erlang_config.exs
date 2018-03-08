# A script to get various config flags
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

  def main(["help"]) do
    IO.puts "Erlang Config\n"
    IO.puts "This script returns paths for various erlang libraries."
    IO.puts "Usage: elixir script/erlang_config.exs [ erts | ei | root | help]\n"
    IO.puts "'root' : the root directory of the erlang install."
    IO.puts "'ei' : the location of the Erlang Interface libraries."
    IO.puts "'erts' : the location of the Erlang Runtime System libraries."
    IO.puts "'help' : shows the help text you are looking at now."
  end

  def main(_args) do
    IO.puts "Usage: elixir script/erlang_config.exs [ erts | ei | root | help]"
  end

  defp root do
    :code.root_dir()
  end
end

ErlangConfig.main(System.argv)
