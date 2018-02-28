defmodule Complex do
	@moduledoc """
		An example of NIF usage. This calls native functions from
		the Gnu Scientific Library's complex number code.
	"""
	@on_load :init

	def init do
		:ok = :erlang.load_nif("./priv/bin/complex_nif", 0)
	end

	@doc "Returns an opaque reference to the complex number a + ib"
	def new({a,b})do
		new(a, b)
	end

  @doc "Returns an opaque reference to the complex number a + ib"
	def new(_a,_b) do
		exit(:nif_library_not_loaded)
	end

	@doc "Returns a tuple {a,b} representing the complex number."
	def to_tuple(_complex) do
		exit(:nif_library_not_loaded)
	end

	@doc "Returns the absolute value (magnitude or modulus) of a complex number."
	def abs(_complex) do
		exit(:nif_library_not_loaded)
	end

	@doc "Returns the argument (angle or phase) of a complex number."
	def arg(_complex) do
		exit(:nif_library_not_loaded)
	end
end
