defmodule Mix.Tasks.Priv.Clean do
  use Mix.Task

  @shortdoc "Cleans the compiled C code in priv."
  def run(_args) do
    File.cd("priv")
    System.cmd("make", ["clean"])
  end
end
