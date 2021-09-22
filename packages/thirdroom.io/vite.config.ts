import { defineConfig } from "vite";
import reactRefresh from "@vitejs/plugin-react-refresh";

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [reactRefresh()],
  server: {
    proxy: {
      "/_matrix": "https://matrix.thirdroom.io/",
    },
  },
  resolve: {
    alias: {
      "@thirdroom/core": "@thirdroom/core/src",
    },
  },
  define: {
    "process.env.NODE_ENV": JSON.stringify(process.env.NODE_ENV),
  },
});
