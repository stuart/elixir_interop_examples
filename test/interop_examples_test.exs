defmodule InteropExamplesTest do
  use ExUnit.Case

  doctest InteropExamples
  doctest Complex
  doctest EiPortExample
  doctest PortDriverExample
  doctest PortExample

  # This starts the C Node and sets our tests to
  # run on a node with the correct cookie.
  @root_dir File.cwd!
  @cnode    Path.join(~w(#{@root_dir} priv bin cnode))

  :os.cmd('#{@cnode} 6000')

  {:ok, _pid} = Node.start(:test@localhost, :shortnames)
  Node.set_cookie :chocolatecookie
  doctest CNodeExample
end
