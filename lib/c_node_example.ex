defmodule CNodeExample do
  def square(x) do
    call_cnode({:square, x})
  end

  def cube(x) do
    call_cnode({:cube, x})
  end

  def call_cnode(msg) do
    send({:any, :cnode@cannelloni}, {:call, self(), msg})
    receive do
      {:cnode, result} ->
        result
    end
  end
end
