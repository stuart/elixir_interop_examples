defmodule InteropExamplesTest do
  use ExUnit.Case

  @root_dir       File.cwd!
  @cnode        Path.join(~w(#{@root_dir} priv bin cnode))

  :os.cmd('#{@cnode} 6000')

  {:ok, _pid} = Node.start(:test@localhost, :shortnames)
  Node.set_cookie :chocolatecookie

  doctest InteropExamples
  doctest Complex
  doctest EiPortExample
  doctest PortDriverExample
  doctest PortExample
  doctest CNodeExample
end
