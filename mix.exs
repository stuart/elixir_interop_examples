defmodule InteropExamples.Mixfile do
  use Mix.Project

  def project do
    [app: :interop_examples,
     version: "0.1.0",
     elixir: "~> 1.3",
     build_embedded: Mix.env == :prod,
     start_permanent: Mix.env == :prod,
     deps: deps()]
  end

  # Configurat ion for the OTP application
  #
  # Type "mix help compile.app" for more information
  def application do
    [ mod: {InteropExamples, []},
      applications: [:logger]]
  end

  # Dependencies can be Hex packages:
  #
  #   {:mydep, "~> 0.3.0"}
  #
  # Or git/path repositories:
  #
  #   {:mydep, git: "https://github.com/elixir-lang/mydep.git", tag: "0.1.0"}
  #
  # Type "mix help deps" for more examples and options
  defp deps do
    [
      {:binaries, path: "./priv", compile: "make all", clean: "make clean", app: false}
    ]
  end
end
