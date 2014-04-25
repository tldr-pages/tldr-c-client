require "formula"

class Tldr < Formula
  homepage "https://github.com/tldr-pages/tldr"
  url "https://github.com/Leandros/tldr-cpp-client/archive/0.1.tar.gz"
  sha1 "650a6cb038c60f39537da8ed3c44aeab1840803f"

  depends_on "curl"
  def install
    system "make"
    bin.install "tldr"
  end

  test do
    system "tldr tar"
  end
end
